node('master') {
    stage('gcc') {
        node {
            checkout scm

            docker_image("gcc").inside {
                sh 'ls'
                sh 'rm -fdr build'
                dir('build') {
                    sh 'cmake -DENABLE_TESTS=ON ..'
                    sh 'cmake --build .'
                    sh 'ctest'
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
