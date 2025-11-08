# Attest

Lightweight unit testing library for C and C++. I'm making this because I'm too lazy to install a proper heavyweight unit testing framework for my projects. I intend for this to be super simple to drop in to projects, as well as for use in CI/CD pipelines or automated test runs.

DONE:
- initial header only implementation
- basic test stuff compatible with C
- got it to work with c++ stl containers
- CLI arguments like --quiet and --filter, to control what tests are run and the output
- colours

TO DO:
- more CLI arguments, like --list to list all registered tests
- continue ensuring c++ compatibility
- maybe add an option to get test results in json format, which could be used to make nice looking dashboards
- eventually remove all libc dependencies - make it possible to run on bare metal on any OS. I would need to figure out how to abstract away writing to stdout.
