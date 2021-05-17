Summary: Graphical interface to DB-All.e databases
Name: provami
Version: 1.7
Release: 3
License: GPL
Group: Applications/Meteo
Source0: https://github.com/arpa-simc/%{name}/archive/v%{version}-%{release}.tar.gz#/%{name}-%{version}-%{release}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot

%if 0%{?rhel} == 7
%define python3_vers python36
# to have python 3.6 interpreter
BuildRequires: python3-rpm-macros >= 3-23
%else
%define python3_vers python3
%endif

BuildRequires: gcc-c++
BuildRequires: cmake
BuildRequires: libdballe-devel >= 7.30
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtwebkit-devel
BuildRequires: %{python3_vers}-dballe
BuildRequires: %{python3_vers}-numpy
# Waiting for issue 120
#Requires: rpy
Obsoletes: provami-qt

%description

 provami is a GUI application to visualise and navigate DB-All.e databases.
 It also allows to perform simple editing tasks, and to graphically select and
 export data subsets.


%prep
%setup -q -n %{name}-%{version}-%{release}

%build

%cmake .

%if 0%{?fedora} >= 32
# https://fedoraproject.org/wiki/Changes/CMake_to_do_out-of-source_builds
%cmake_build
%else
%make_build
%endif

%install
[ "%{buildroot}" != / ] && rm -rf "%{buildroot}"

%if 0%{?fedora} >= 32
# https://fedoraproject.org/wiki/Changes/CMake_to_do_out-of-source_builds
%cmake_install
%else
%make_install
%endif

%check
ctest -V %{?_smp_mflags}

%clean
[ "%{buildroot}" != / ] && rm -rf "%{buildroot}"

%files
%defattr(-,root,root,-)
%{_datadir}/provami/mapview
%{_bindir}/provami-qt
%doc %{_mandir}/man1/provami-qt.1.gz


%post


%postun

%changelog
* Mon May 17 2021 Daniele Branchini <dbranchini@arpae.it> - 1.7-3
- bogus release to compile against new dballe

* Wed May  5 2021 Daniele Branchini <dbranchini@arpae.it> - 1.7-2
- fixed builds for F33/F34

* Mon Feb 10 2020 Daniele Branchini <dbranchini@arpae.it> - 1.7-1
- ported to new dballe

* Thu Dec 19 2019 Daniele Branchini <dbranchini@arpae.it> - 1.6-5
- bogus release to compile against new dballe

* Mon Oct 28 2019 Daniele Branchini <dbranchini@arpae.it> - 1.6-4
- bogus release to compile against new dballe

* Tue Apr 16 2019 Daniele Branchini <dbranchini@arpae.it> - 1.6-3
- moving to python 3.6 on Centos7

* Wed Feb 27 2019 Daniele Branchini <dbranchini@arpae.it> - 1.6-2
- fixed dependencies

* Wed Feb 27 2019 Daniele Branchini <dbranchini@arpae.it> - 1.6-1
- fixed DB::connect issue

* Wed Feb 27 2019 Daniele Branchini <dbranchini@arpae.it> - 1.5-1
- ported to new dballe 8 API

* Wed Mar 28 2018 Daniele Branchini <dbranchini@arpae.it> - 1.4-2
- fixed #28

* Wed Feb 14 2018 Daniele Branchini <dbranchini@arpae.it> - 1.3-1
- fixed #27
- change rpm name for automating build from github

* Tue Apr 26 2016 Daniele Branchini <dbranchini@arpa.emr.it> - 1.2-1%{dist}
- new dballe build

* Tue Oct 27 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4832%{dist}
- fixed github issues 5, 9, 8, 12, 15, 17

* Fri Sep 25 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4831%{dist}
- fixed issue https://github.com/ARPA-SIMC/provami/issues/2

* Thu Sep 24 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4830%{dist}
- added man page and help options

* Wed Apr 22 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4687%{dist}
- rebuild con cmake

* Tue Feb 17 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4508%{dist}
- aggiunta gestione summary

* Thu Feb 12 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4497%{dist}
- aggiunta una riga che mostra gli estremi di date e il numero di risultati stimati
- aggiunto un secondo bottone per fare refresh ricalcolando anche le statistiche
- fatto in modo che il refresh blocchi l\'interfaccia per il meno tempo possibile

* Thu Feb 5 2015 Daniele Branchini <dbranchini@arpa.emr.it> - qt1.0.0-4479%{dist}
- first build
