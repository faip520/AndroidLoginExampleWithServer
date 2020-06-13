FROM faip/single-login:1.0.0
RUN sudo service mysql restart
CMD [ "./single_login_src/SingleLoginServer/bazel-bin/examples/greeter_server" ]