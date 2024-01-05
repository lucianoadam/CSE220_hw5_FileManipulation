FROM mcr.microsoft.com/devcontainers/universal:2
RUN curl -o criterion.tar.xz -L https://github.com/Snaipe/Criterion/releases/download/v2.4.1/criterion-2.4.1-linux-x86_64.tar.xz && xz -d criterion.tar.xz && tar xvf criterion.tar && cd criterion-2.4.1 && sudo cp -r include /usr/local/ && sudo cp -r lib /usr/local/ && sudo ldconfig
