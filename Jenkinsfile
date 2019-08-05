def compilers = ["arm-linux-gnueabihf"]

def jobs = compilers.collectEntries {
    ["${it}": job(it)]
}

node('master') {
    checkout scm
    parallel jobs
}

def job(compiler) {
    return {
        docker_image(compiler).inside {
            cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON -DCMAKE_TOOLCHAIN_FILE=/usr/Toolchain-arm-linux.cmake', installation: 'InSearchPath', steps: [[withCmake: true, args: '--target chapro-openmha-plugin']]
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
