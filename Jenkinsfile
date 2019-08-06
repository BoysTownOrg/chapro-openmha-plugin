node('master') {
    stage('gcc') {
        node {
            checkout scm

            docker_image("gcc").inside {
                sh 'ls'
                sh 'mkdir build'
                sh 'cd build'
                sh 'cmake ..'
                sh 'cmake --build .'
                sh 'ctest .'
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
