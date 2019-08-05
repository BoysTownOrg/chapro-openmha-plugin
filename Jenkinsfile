def compilers = ["gcc"]

def jobs = compilers.collectEntries {
    ["${it}": job(it)]
}

node('master') {
    stage('builds-with-testing') {
        parallel jobs
    }

    stage('arm-build') {
        node {
            checkout scm

            docker_image("arm-linux-gnueabihf").inside {
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DCMAKE_TOOLCHAIN_FILE=/usr/Toolchain-arm-linux.cmake', installation: 'InSearchPath', steps: [[withCmake: true, args: '--target chapro-openmha-plugin']]
            }
        }
    }
}

def job(compiler) {
    return {
        node {
            checkout scm
            
            docker_image(compiler).inside {
                def cmake_args = [
                    gcc: "", 
                    clang: "", 
                    mingw: 
                        "-DCMAKE_TOOLCHAIN_FILE=/usr/Toolchain-mingw32.cmake " +
                        "-DCMAKE_CROSSCOMPILING_EMULATOR=wine64",
                    osxcross: ""
                ]
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON ' + cmake_args[compiler], installation: 'InSearchPath', steps: [[withCmake: true]]
                ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
