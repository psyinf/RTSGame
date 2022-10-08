from conans import ConanFile
from conan.tools.cmake import CMake

class RTSGame(ConanFile):
    name = "RTSGame"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = [("openscenegraph/3.6.5")
				,("nlohmann_json/3.11.2")
                ,("glog/0.6.0")
                ,("gtest/1.12.1")
                ,("qt/5.15.6")
                ,("boost/1.79.0")
                ,("double-conversion/3.2.1")
                ]
    generators = "cmake_find_package_multi"
    
    def configure(self):
        self.options['openscenegraph'].shared = True
        self.options['glog'].shared = True
        self.options['qt'].shared = True
        self.options['qt'].multiconfiguration = False
        self.options['qt'].multiconfiguration = False
        
    def imports(self):    
        self.copy("*.dll", "bin", "bin")
        