node('master') {

    stage('arm-linux-gnueabihf build') {
        node {
            checkout_scm()

            docker_image('arm-linux-gnueabihf').inside {
                execute_command_line('bbb/build')
            }
        }
    }
}

def checkout_scm() {
    checkout scm
}

def cmake_generate_build_with_tests() {
    cmake_generate_build('-DENABLE_TESTS=ON')
}

def cmake_generate_build(flags) {
    execute_command_line('cmake ' + flags + ' ..')
}

def cmake_generate_build_with_toolchain(toolchain) {
    cmake_generate_build('-DCMAKE_TOOLCHAIN_FILE=../' + toolchain)
}

def cmake_build_target(target) {
    cmake_build('--target ' + target)
}

def cmake_build(flags = '') {
    execute_command_line('cmake --build . ' + flags)
}

def execute_tests() {
    execute_command_line('ctest')
}

def execute_command_line(what) {
    sh what
}

def docker_image(compiler) {
    return docker.build(compiler, './docker/' + compiler)
}
