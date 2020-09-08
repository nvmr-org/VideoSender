node {
	stage('Clean'){
			cleanWs()
	}
	stage('Checkout'){
			fileOperations([folderCreateOperation('source')])
			dir('source'){
				def scmVars = checkout scm
				env.GIT_COMMIT = scmVars.GIT_COMMIT
			}
	}
	stage("Build"){
		def repoHook="26be5ef3-dc1e-4b05-9d48-bc01c56ba708"

		configFileProvider([configFile(fileId: "${repoHook}", targetLocation: 'hookdir/D21-repo-hook')]){
		debianPbuilder additionalBuildResults: '', 
			architecture: 'armhf', 
			components: '', 
			distribution: 'buster', 
			keyring: '', 
			mirrorSite: 'http://deb.debian.org/debian/', 
			pristineTarName: ''
		}
	} //stage

	stage("Add to repo if master"){
		if( env.BRANCH_NAME == "master" ){
			aptlyPublish includeSource: true, removeOldPackages: true, repositoryName: "nightly-nvmr"
		}
	}
}
