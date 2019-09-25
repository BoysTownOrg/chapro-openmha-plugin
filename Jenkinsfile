node('master') {
    run_stage_inside_docker_image_generic_build_directory(
        'gcc build all and test', 
        'gcc',
        { compile_all_and_test() }
    )

    run_stage_inside_docker_image_generic_build_directory(
        'arm-linux-gnueabihf build plugins',
        'arm-linux-gnueabihf',
        { cross_compile_plugins() }
    )

    run_stage(
        'gcc build all and arm-linux-gnueabihf build plugins', 
        {
            run_inside_docker_image(
                'both',
                {
                    run_inside_directory(
                        'build-gcc', 
                        { 
                            cmake_generate_build() 
                            build_plugins()    
                        }
                    )

                    run_inside_directory(
                        'build-arm-linux', 
                        { cross_compile_plugins() }
                    )
                }
            )
        }
    )
}

def run_stage_inside_docker_image_generic_build_directory(stage_, image, f) {
    run_stage_inside_docker_image_directory(stage_, image, 'build', f)
}

def run_stage_inside_docker_image_directory(stage_, image, directory, f) {
    run_stage(
        stage_,
        { run_inside_docker_image_directory(image, directory, f) }
    )
}

def run_stage(stage_, f) {
    stage(stage_) {
        node {
            checkout scm
            f()
        }
    }
}

def run_inside_docker_image_directory(image, directory, f) {
    run_inside_docker_image(
        image,
        { run_inside_directory(directory, f) }
    )
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

def docker_image(compiler) {
    return docker.build(compiler, './docker/' + compiler)
}

def compile_all_and_test() {
    cmake_generate_build_with_tests()
    cmake_build()
    execute_tests()
}

def cmake_generate_build_with_tests() {
    cmake_generate_build('-DENABLE_TESTS=ON')
}

def cmake_generate_build(flags = '') {
    execute_command_line('cmake ' + flags + ' ..')
}

def execute_command_line(what) {
    sh what
}

def cmake_build(flags = '') {
    execute_command_line('cmake --build . ' + flags)
}

def execute_tests() {
    execute_command_line('ctest')
}

def cross_compile_plugins() {
    cmake_generate_build_with_toolchain('Toolchain-arm-linux-gnueabihf.cmake')
    build_plugins()
}

def cmake_generate_build_with_toolchain(toolchain) {
    cmake_generate_build('-DCMAKE_TOOLCHAIN_FILE=../' + toolchain)
}

def build_plugins() {
    cmake_build_target('chapro-openmha-plugin')
}

def cmake_build_target(target) {
    cmake_build('--target ' + target)
}
