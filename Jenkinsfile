pipeline {
    agent any

    stages {
        stage ('Build') {
            steps {
 	        cmakeBuild buildDir: 'build', cleanBuild: true, installation: 'InSearchPath', steps: [[args: '--target chapro-openmha-plugin', withCmake: true]]
	    }
        }

        stage ('Test') {
	    steps {
                ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }
}
