/* \author Geoffrey Biggs */


#include <visp/vpConfig.h>

#include <sys/time.h> // to get time
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <math.h>

#include <Eigen/Dense>

#include <libgaitan/plane.h>
#include <libgaitan/conversion.h>
#include <libgaitan/kinect.h>

#if (defined (VISP_HAVE_X11) || defined(VISP_HAVE_GTK) || defined(VISP_HAVE_OPENCV) || defined(VISP_HAVE_GDI))
#include <visp/vpImage.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpTime.h>
#include <visp/vpImageIo.h>
#include <visp/vpRGBa.h>

using namespace gaitan;

#include <iostream>

#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/point_types.h>
//#include <pcl/io/pcd_io.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>


void useInput (int argc, char** argv, std::string& path, std::string &pattern, int & nbIm)
{
  
  pattern = "depth_%07d.pfm";
 
  // get the path name
	if (argc>1){
		path = argv[1];
	}
	else {
    path ="/home/dune/Documents/data/kinect/essai1";
  }
  
  // get the number of images to treat
  if (argc>2){
    nbIm = atoi(argv[2]);
    }
  else  nbIm = 50;  
}



boost::shared_ptr<pcl::visualization::PCLVisualizer> shapesVis (pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr cloud,
float & a, float & b, float & c, float & d, std::string &id)
{
  // --------------------------------------------
  // -----Open 3D viewer and add point cloud-----
  // --------------------------------------------
  boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
  viewer->setBackgroundColor (0, 0, 0);
  pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(cloud);
  viewer->addPointCloud<pcl::PointXYZRGB> (cloud, rgb, id);
  viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3,id);
  viewer->addCoordinateSystem (1.0);
  viewer->initCameraParameters ();


  //---------------------------------------
  //-----Add shapes at other locations-----
  //---------------------------------------
  pcl::ModelCoefficients coeffs;
  coeffs.values.push_back (a);
  coeffs.values.push_back (b);
  coeffs.values.push_back (c);
  coeffs.values.push_back (d);
  viewer->addPlane (coeffs, "plane");

  return (viewer);
}



// --------------
// -----Main-----
// -------------
int
main (int argc, char** argv)
{
  // use the shell input
  std::string path, pattern, fullPath;
  int nbIm(0);
  useInput(argc, argv, path, pattern, nbIm);
  
  // create the kinect sensor model to handle the images
  Kinect * kinect= new Kinect();
  kinect->print();
  /*
  
  // create the pcl viewer
  boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));   
  viewer->addCoordinateSystem (1.0);
  viewer->initCameraParameters ();
  // ground plane : assumed to be the same for all images
  Plane plane, gPlane;      
      
  // read the images 
  std::string cloudId;
  for(int index=0; index<nbIm ; index++){
  //int index=1;  
    // prompt message
    std::cout << "----------------------" << std::endl ;
    std::cout << "Image " << index << std::endl ;
    std::cout << "----------------------" << std::endl ;
        
    // get the point cloud
    Eigen::MatrixXf pointCloud = kinect->pointCloud(path,index+10);
 
    // find the position of the kinect with regards to the ground
    double confidence = 0.03;
    if(index==0){
      plane = kinect->ground(pointCloud, confidence/2);
      gMk = plane.computeTransformation();
    }
    
    // divide the set of points in two parts the point in the plane and
    // the points out of the plane
    Eigen::MatrixXf ptsIn(pointCloud), ptsOut(3,1), gPtsIn, gPtsOut ;
    plane.inlierSelection(ptsIn, ptsOut, confidence);
    
    std::cout << "POINT OUT "<< ptsOut.rows() << std::endl;
    
    gPtsIn  = changeFrame(ptsIn,gMk);  
    gPtsOut = changeFrame(ptsOut,gMk);  
      
    //populate cloud for visualisation    
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr colorCloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    Conversion::convert(gPtsIn,colorCloud, 255, 0, 255);
    Conversion::convert(gPtsOut,colorCloud, 0, 255,255);
    
      
    // segment the points to get the two feets
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    Conversion::convert(gPtsOut,cloud);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFiltered(new pcl::PointCloud<pcl::PointXYZ>);
    std::vector<pcl::PointIndices> clusterIndices;
    kinect->segment(cloud, cloudFiltered,clusterIndices,500, 10000, confidence);


    // Creating the Clusters
    int j(0);
    for (std::vector<pcl::PointIndices>::const_iterator it = clusterIndices.begin (); it != clusterIndices.end (); ++it){
      pcl::PointCloud<pcl::PointXYZ>::Ptr cloudCluster (new pcl::PointCloud<pcl::PointXYZ>);
      for (std::vector<int>::const_iterator pit = it->indices.begin (); pit != it->indices.end (); pit++)
        cloudCluster->points.push_back (cloudFiltered->points[*pit]); 
        cloudCluster->width = cloudCluster->points.size ();
        cloudCluster->height = 1;
        cloudCluster->is_dense = true;
         
        if(j==0) 
           Conversion::convert(cloudCluster,colorCloud, 0, 0,255);
        else if (j==1)
           Conversion::convert(cloudCluster,colorCloud, 255, 0,0);  
        else if (j==2)
           Conversion::convert(cloudCluster,colorCloud, 0, 255,0);    
        
        std::cout << "PointCloud representing the Cluster: " << cloudCluster->points.size () << " data points." << std::endl;
        j++;
      }
        
    std::cout << "There are " << j << " clusters " << endl;
    
    // viewer
    // create the id of the current cloud and of the previous one
    
    if (index>=1) viewer->removePointCloud(cloudId);
    
    std::stringstream out;
    out <<  "cloud_" <<index;        
    cloudId = out.str();
         
    std:: cout << " Insertion des points" << std::endl;
    pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(colorCloud);
    viewer->addPointCloud<pcl::PointXYZRGB> (colorCloud, rgb,cloudId);
    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, cloudId);

      
    int elapse(0);
    //while (!viewer->wasStopped ())
    while (elapse < 3)
    {
      viewer->spinOnce (100);
      boost::this_thread::sleep (boost::posix_time::microseconds (1000000));
      elapse ++;
     std::cout << elapse << std::endl;
    }
  }*/
}
#else
int
main(){
	std::cout << "You should install a video device (X11, GTK, OpenCV, GDI) to run this example" << std::endl;
}
#endif
