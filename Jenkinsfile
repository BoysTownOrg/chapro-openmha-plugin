node('master') {
    stage('gcc build and test') {
        node {
            checkout_scm()

            docker_image('gcc').inside {
                run_inside_directory('build', compile_all_and_test)
            }
        }
    }

    stage('arm-linux-gnueabihf build') {
        node {
            checkout_scm()

            docker_image('arm-linux-gnueabihf').inside {
                dir('build') {
                    cross_compile_plugins()
                }
            }
        }
    }

    stage('build both') {
        node {
            checkout_scm()

            docker_image('both').inside {
                dir('build-gcc') {
                    cmake_generate_build()
                    build_plugins()
                }

                dir('build-arm-linux') {
                    cross_compile_plugins()
                }
            }
        }
    }
}

def compile_all_and_test() {
    cmake_generate_build_with_tests()
    cmake_build()
    execute_tests()
}

def run_inside_directory(directory, f) {
    dir(directory) {
        f()
    }
}

def run_inside_docker_image(image, f) {
    docker_image(image).inside {
        f()
    }
}

def cross_compile_plugins() {
    cmake_generate_build_with_toolchain(
        'docker/arm-linux-gnueabihf/Toolchain-arm-linux-gnueabihf.cmake'
    )
    build_plugins()
}

def build_plugins() {
    cmake_build_target('chapro-openmha-plugin')
    cmake_build_target('chapro-afc-openmha-plugin')
}

def checkout_scm() {
    checkout scm
}

def cmake_generate_build_with_tests() {
    cmake_generate_build('-DENABLE_TESTS=ON')
}

def cmake_generate_build(flags = '') {
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
