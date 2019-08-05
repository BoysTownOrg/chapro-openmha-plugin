def compilers = ["arm-linux-gnueabihf"]

def jobs = compilers.collectEntries {
    ["${it}": job(it)]
}

node('master') {
    checkout scm
    parallel jobs
}

def job(compiler) {
    return node {
        docker_image(compiler).inside {
            cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
            ctest installation: 'InSearchPath', workingDir: 'build'
	}
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
