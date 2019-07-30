pipeline {
    agent any

    stages {
        stage ('Build') {
            steps {
 	        cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath'
	    }
        }

        stage ('Test') {
	    steps {
                ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }
}
