def compilers = ["arm-linux-gnueabihf"]

def jobs = compilers.collectEntries {
    ["${it}": job(it)]
}

parallel jobs

def job(compiler) {
    return {
        docker_image(compiler).inside {
            checkout scm
            cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
            ctest installation: 'InSearchPath', workingDir: 'build'
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
