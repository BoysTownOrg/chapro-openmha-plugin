node('master') {
    stage('arm-linux-gnueabihf') {
        node {
            checkout scm

            stage('change permissions of build script') {
                sh 'ls'
                sh 'chmod +x ./build'
            }

            docker_image("arm-linux-gnueabihf").inside {
                sh 'ls'
                sh './build'
            }
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
