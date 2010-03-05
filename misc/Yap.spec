#%define _unpackaged_files_terminate_build 0
#%undefine __check_files

Name: Yap
Summary: Prolog Compiler
Version: 6.0.3
Packager: Vitor Santos Costa <vitor@cos.ufrj.br>
Release: 1
Source: http://www.dcc.fc.up.pt/~vsc/Yap/%{name}-%{version}.tar.gz
License: Perl Artistic License
Provides: yap
Requires: readline
Group: Development/Languages
URL: http://www.ncc.up.pt/~vsc/Yap
Prefix: /usr
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
A high-performance Prolog compiler developed at LIACC,
Universidade do Porto. The Prolog engine is based in the WAM (Warren
Abstract Machine), with several optimizations for better
performance. YAP follows the Edinburgh tradition, and is largely
compatible with the ISO-Prolog standard and with Quintus and SICStus
Prolog.

%prep

%setup -q

%build
./configure --prefix=/usr --enable-max-performance --enable-depth-limit
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
mkdir -p $RPM_BUILD_ROOT/usr/share/info
make DESTDIR=$RPM_BUILD_ROOT install_info

%post
/sbin/install-info  --quiet /usr/share/info/yap.info --section "Programming Languages" /usr/share/info/dir
/sbin/install-info  --quiet /usr/share/info/pillow_doc.info --section "Programming Languages" /usr/share/info/dir

%postun
/sbin/install-info  --quiet --delete yap.info /usr/share/info/dir
/sbin/install-info  --quiet --delete pillow_doc.info /usr/share/info/dir

rm -f /usr/info/yap.info*

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/file.list.%{name}

%files
%defattr(-,root,root,-)
%doc README*
%doc INSTALL
%doc changes4.3.html
%doc docs/yap.tex
/usr/bin/yap
/usr/lib/Yap/
/usr/lib/libYap.a
/usr/include/Yap/
/usr/share/Yap/
/usr/share/info/yap.info*
/usr/share/info/pillow_doc.info*

%changelog

