class Mysqlxx < Formula
  desc "C++ wrapper for MySQL's C API"
  homepage "https://tangentsoft.com/mysqlpp/home"
  url "https://tangentsoft.com/mysqlpp/releases/mysql++-3.2.5.tar.gz"
  sha256 "b780beeb3a9cd9ce6a9043028527484df8e822c58c5274d4d67ec5ba2fc0a778"

  depends_on "mysql-client"

  def install
    mysql = Formula["mysql-client"]
    system "./configure", "--disable-dependency-tracking",
                          "--prefix=#{prefix}",
                          "--with-field-limit=40",
                          "--with-mysql-lib=#{mysql.opt_lib}",
                          "--with-mysql-include=#{mysql.opt_include}/mysql"
    system "make", "install"
  end

  test do
    (testpath/"test.cpp").write <<~EOS
      #include <mysql++/cmdline.h>
      int main(int argc, char *argv[]) {
        mysqlpp::examples::CommandLine cmdline(argc, argv);
        if (!cmdline) {
          return 1;
        }
        return 0;
      }
    EOS
    system ENV.cxx, "test.cpp", "-I#{Formula["mysql-client"].opt_include}/mysql",
                    "-L#{lib}", "-lmysqlpp", "-o", "test"
    system "./test", "-u", "foo", "-p", "bar"
  end
end
