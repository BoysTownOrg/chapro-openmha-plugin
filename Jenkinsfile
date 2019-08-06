node('master') {
    stage('gcc') {
        node {
            checkout scm

            docker_image("gcc").inside {
                dir('build') {
                    sh 'cmake -DENABLE_TESTS=ON ..'
                    sh 'cmake --build .'
                    sh 'ctest'
                }
            }
        }
    }
    
    stage('mingw') {
        node {
            checkout scm

            docker_image("mingw").inside {
                dir('build') {
                    sh 'cmake -DCMAKE_TOOLCHAIN_FILE=../docker/mingw/Toolchain-mingw32.cmake ..'
                    sh 'cmake --build . --target chapro-openmha-plugin'
                }
            }
        }
    }

    stage('arm-linux-gnueabihf') {
        node {
            checkout scm

            docker_image("arm-linux-gnueabihf").inside {
                sh 'bbb/build'
            }
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
