node('master') {
    stage('gcc') {
        node {
            checkout scm

            docker_image("gcc").inside {
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
                ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }

    stage('mingw') {
        node {
            checkout scm

            docker_image("mingw").inside {
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DCMAKE_TOOLCHAIN_FILE=/usr/Toolchain-mingw32.cmake', installation: 'InSearchPath', steps: [[withCmake: true, args: '--target chapro-openmha-plugin']]
            }
        }
    }

    stage('arm-linux-gnueabihf') {
        node {
            checkout scm

            docker_image("arm-linux-gnueabihf").inside {
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DCMAKE_TOOLCHAIN_FILE=/usr/Toolchain-arm-linux.cmake', installation: 'InSearchPath', steps: [[withCmake: true, args: '--target chapro-openmha-plugin']]
            }
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
