docker build -t systemc-learning:latest --build-arg USER_ID=$(id -u ${USER}) --build-arg GROUP_ID=$(id -g ${USER}) .
docker run -it -d --rm -v $(pwd):/home/systemc-learning systemc-learning:latest