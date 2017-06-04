
# Build & Install OCS
Build Dependencies 
IMS HSS Depends on following Libraries. Make sure that you have built /installed all of the following libraries before building OCS 
- libamps (AMPS library)
- libxml2 (XML Libraray)
- libdiameter (Diameter Base Stack library)
Get Source Code
mkdir imsocs 
git clone https://github.com/inamkhosa/imsocs.git
Compile and Install
# cd imsocs 
# ./autogen.sh --prefix=/usr –datarootdir=/etc 
For debugging with gdb, following command may be used:
# ./autogen.sh --prefix=/usr --datarootdir=/etc --enable-debug=yes 
For release compilation, following command may be used:
# ./autogen.sh --prefix=/usr --datarootdir=/etc --enable-debug=no --enable-opt=8
Note: If –prefix and –datarootdir are not given then HSS will install to default directories. Default directories for installation are /usr/local/bin for binary files , /usr/local/lib for libraries and /usr/local/hss for configuration files.
Compile and Install
# make 
# make install
Not: if you use different directories for installation rather than default directories then you should add you binary file path to environment variable PATH and libraries file to environment variable LD_LIBRARY_PATH.
Configure OCS 
The configuration file ocs_config.xml is used to provide the configuration options for running the OCS application. The default path for installation of this configuration file is /usr/local/share/onlinecharging 
#vi /usr/local/share/onlinecharging/ocs_config.xml 
Change the IPs and Ports to reflect to your system IP and Port. 
Start OCS 
The name of application installed for executing the OCS is 'onlinecharging'. In Linux the default path for its installation is /usr/local/bin. Use the following command for starting the OCS.. 
# onlinecharging

# imsocs is licensed under Creative Common License (https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode). 
You can look at https://creativecommons.org/licenses/by-nc-sa/4.0/ for non-commercial usage.
