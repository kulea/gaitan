#--------------------------------------------
#
# LISTE DES EXECUTABLES
#
#--------------------------------------------

SET (SOURCE
testPlane.cpp
testBox.cpp
#testShape.cpp
testHomogeneousMatrix.cpp
)

# rule for binary build
FOREACH(source ${SOURCE})
  # Compute the name of the binary to create
  GET_FILENAME_COMPONENT(binary ${source} NAME_WE)

  # From source compile the binary and add link rules
  ADD_EXECUTABLE(${binary} ${source})
  TARGET_LINK_LIBRARIES(${binary} 
    ${GAITAN_INTERN_LIBS} ${GAITAN_EXTERN_LIBS}
    )
  # Add test
 ADD_TEST(${binary} ${binary})

ENDFOREACH(source)


add_subdirectory(PCL)
