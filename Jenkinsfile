node('master') {
    stage('gcc build and test') {
        node {
            checkout scm

            docker_image('gcc').inside {
                dir('build') {
                    cmake_generate_build('-DENABLE_TESTS=ON')
                    cmake_build()
                    execute_command_line('ctest')
                }
            }
        }
    }
    
    stage('mingw build') {
        node {
            checkout scm

            docker_image('mingw').inside {
                dir('build') {
                    cmake_generate_build('-DCMAKE_TOOLCHAIN_FILE=../docker/mingw/Toolchain-mingw32.cmake')
                    cmake_build('--target chapro-openmha-plugin')
                }
            }
        }
    }

    stage('arm-linux-gnueabihf build') {
        node {
            checkout scm

            docker_image('arm-linux-gnueabihf').inside {
                execute_command_line('bbb/build')
            }
        }
    }
}

def cmake_generate_build(flags) {
    execute_command_line('cmake ' + flags + ' ..')
}

def cmake_build(flags = '') {
    execute_command_line('cmake --build . ' + flags)
}

def execute_command_line(what) {
    sh what
}

def docker_image(compiler) {
    return docker.build(compiler, './docker/' + compiler)
}
