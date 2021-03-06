#ifndef GAITAN_SHAPE_H
#define GAITAN_SHAPE_H


#include<Eigen/Dense>

using namespace Eigen;

namespace gaitan
{
  class Shape
  {  
	protected:
	VectorXf parameters;
	
    public:	
     Shape();
     Shape(const VectorXf & param);
     virtual ~Shape();
     
     VectorXf getParameters();
     void print();
     
     /*!
      * FindParameters : fit the 3D parametric shape to the pts
      * set the parameters of the shape to the fitted one
      */
     virtual int findParameters(const MatrixXf & pts )=0;
     virtual VectorXf computeDistance(const MatrixXf & pts)=0;
    
    protected: 
     virtual float computeDistance(const double &X, const double &Y, const double & Z)=0;
      
  };
}
#endif // shape_H
