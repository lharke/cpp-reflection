add_library(reflection
  ../include/reflection/metaobject.h
  ../include/reflection/function.h
  ../include/reflection/constructor.h
  ../include/reflection/method.h
  ../include/reflection/property.h
  ../include/reflection/event.h
  ../include/reflection/reflectable.h
  ../include/reflection/library.h
  ../include/reflection/utility.h
  ../include/reflection/exceptions.h
  reflectable.cpp
  library.cpp
  utility.cpp
)

target_include_directories(reflection PUBLIC
  $<BUILD_INTERFACE:${PROJECT_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(reflection PUBLIC cxx_std_20)
