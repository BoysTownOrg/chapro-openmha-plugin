node('master') {
    stage('arm-linux-gnueabihf') {
        node {
            checkout scm

            docker_image("arm-linux-gnueabihf").inside {
                sh 'ls'
                sh 'chmod +x ./build'
                sh './build'
            }
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
