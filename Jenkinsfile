def compilers = ["gcc"]

def jobs = compilers.collectEntries {
    ["${it}": job(it)]
}

node('master') {
    checkout scm
    
    stage('builds-with-testing') {
        parallel jobs
    }

    stage('arm-build') {
        docker_image("arm-linux-gnueabihf").inside {
            cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DCMAKE_TOOLCHAIN_FILE=/usr/Toolchain-arm-linux.cmake', installation: 'InSearchPath', steps: [[withCmake: true, args: '--target chapro-openmha-plugin']]
        }
    }
}

def job(compiler) {
    return {
        docker_image(compiler).inside {
            cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true, args: '--target chapro-openmha-plugin']]
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}
