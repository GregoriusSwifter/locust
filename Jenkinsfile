#!groovy
node {
    stage 'Checkout'
    checkout scm

    stage 'Clean'
    sh "gmake clean"

    stage 'Build'
    sh "gmake all"

    stage 'Test'
    sh "gmake test"

    stage 'Install'
    sh "gmake install"
}
