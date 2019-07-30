pipeline {
    stages {
        stage ('macOS-chapro-openmha-plugin - Build') {
            steps {
 	        cmakeBuild buildDir: 'build', cleanBuild: true, installation: 'InSearchPath', steps: [[args: '--target chapro-openmha-plugin', withCmake: true]]
	    }
        }
    }
}
