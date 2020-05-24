#include "appweb.h"

//Параметры: --docroot . --http-listen 0.0.0.0:8080
int main(int argc, char **argv) {
    return WRun(argc, argv, [](const Wt::WEnvironment &env) {
        return cpp14::make_unique<AppWeb>(env);
    });
}
