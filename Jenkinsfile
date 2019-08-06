node('master') {
    stage('gcc') {
        node {
            checkout scm

            docker_image("gcc").inside {
                sh 'ls'
                sh 'mkdir build_'
                sh 'cmake -S . -B build_'
                sh 'cmake --build build_'
                sh 'ctest build_'
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
