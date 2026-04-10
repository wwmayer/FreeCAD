<a href="https://codeberg.org/xCAD"><img src="./src/Gui/Icons/freecad.svg" height="100px" width="100px"></a>

### Your own 3D parametric modeler

[Website](https://www.freecad.org) •
[Documentation](https://wiki.freecad.org) •
[Discussions](https://codeberg.org/xCAD/xCAD_discussions/issues) •
[Bug tracker](https://codeberg.org/xCAD/FreeCAD11/issues) •
[Git repository](https://codeberg.org/xCAD/FreeCAD11) •


<img src="./.github/images/F16_MLG_window.png" width="800"/>

Overview
--------

* **Freedom to build what you want**  FreeCAD is an open-source parametric 3D 
modeler made primarily to design real-life objects of any size. 
Parametric modeling allows you to easily modify your design by going back into 
your model history to change its parameters. 

* **Create 3D from 2D and back** FreeCAD lets you to sketch geometry constrained
 2D shapes and use them as a base to build other objects. 
 It contains many components to adjust dimensions or extract design details from 
 3D models to create high quality production-ready drawings.

* **Designed for your needs** FreeCAD is designed to fit a wide range of uses
including product design, mechanical engineering and architecture,
whether you are a hobbyist, programmer, experienced CAD user, student or teacher.

* **Cross platform** FreeCAD runs on Windows, macOS, Linux and *BSD operating systems.

* **Underlying technology**
    * **OpenCASCADE** A powerful geometry kernel, the most important component of FreeCAD
    * **Coin3D library** Open Inventor-compliant 3D scene representation model
    * **Python** FreeCAD offers a broad Python API
    * **Qt** Graphical user interface built with Qt

    
    
History
-------

This repository is forked from the 1.1 FreeCAD version on GitHub, at commit [#1ae8524dce](https://codeberg.org/xCAD/FreeCAD11/commit/1ae8524dce2). The reasons for this forking are numerous : 

* **Data Compatibility**

During the 0.22=>1.0 development period, a single commit on the previous GitHub repository, [#12714](https://github.com/FreeCAD/FreeCAD/pull/12714), made the new data format incompatible with the original data format used since the beginnings of FreeCAD. Some backwards compatibility scripts were introduced, but no forward compatibility was possible. Furthermore, this data incompatibility was subtle and not documented, leading to data corruption.

This data incompatibility lead also to the impossibility to develop a same model across several designers using different FreeCAD versions, eliminating one of FreeCAD's strength to be multi-platform. Since there couldn't be any guarantee that a team of designers using FreeCAD on MacOS, Linux, Windows or *BSD would all use the exact same version, it became instantly impossible to share a complex assembly, limiting usage of FreeCAD to hobbyists, while our goal is to aim for FreeCAD to be equal to the top-tier CAD systems worldwide.

The current 2.0 version is fully backwards and forwards compatible with the old stable maintenance branch of [0.21 maintained on codeberg](https://codeberg.org/xCAD/FreeCAD21). 

* **Development Model**

The introduction of this data format change was not only a technical issue, but the manner in which it was introduced, without any discussion and testing, destroyed the most valuable asset of any free and open-source software : trust. 

The subsequent departure of most of the main developers, either dropping out silently or banned with a loud bang, called for an alternative hosting restoring the successful 20 year old development model, based on code quality and testing, and not on the fast addition of questionable features. 

* **Independence**

The previous hosting platform, GitHub, while open nominally, is actually the private property of a US corporation hostile to free and open-source software (Microsoft). This, combined with the recent injection of private venture capital money by an unknown US startup (Ondsel) and obvious conflicts of interest between some FreeCAD developers and unknown financial backing, made some of us question the true  independence of FreeCAD. Thus, a team of former core FreeCAD developers, including one of its original founders (Werner Mayer), decided to create this fork hosted on a European platform [codeberg.org](https://codeberg.org) (based in Germany) by a non-profit organisation : [Codeberg](https://codeberg.org/about)


The goal of this fork is to maintain the original credo of FreeCAD to be a truly free and open-source software (FOSS) with a strong and lively community. We believe that true engineers need stability and quality before shiny features, and we believe that the external addon ecosystem is as important for FreeCAD as the core software. 


Installing
----------

Precompiled packages for stable releases are available for Windows, macOS and Linux on the
[Releases page](https://codeberg.org/xCAD/FreeCAD11/releases).

On most Linux distributions, FreeCAD is also directly installable from the 
software center application.

Other options are described on the [wiki Download page](https://wiki.freecad.org/Download).


Compiling
---------

FreeCAD requires several dependencies to correctly compile for development and
production builds. The following pages contain updated build instructions for
their respective platforms:

- [Linux](https://wiki.freecad.org/Compile_on_Linux)
- [Windows](https://wiki.freecad.org/Compile_on_Windows)
- [macOS](https://wiki.freecad.org/Compile_on_MacOS)
- [MinGW](https://wiki.freecad.org/Compile_on_MinGW)


Reporting Issues
---------

To report an issue please:

- Consider posting to the [Forum](https://forum.freecad.org), [Discord](https://discord.com/invite/w2cTKGzccC) channel, or [Reddit](https://www.reddit.com/r/FreeCAD) to verify the issue; 
- Search the existing [issues](https://github.com/FreeCAD/FreeCAD/issues) for potential duplicates; 
- Use the most updated stable or [development versions](https://github.com/FreeCAD/FreeCAD-Bundle/releases/tag/weekly-builds) of FreeCAD; 
- Post version info from `Help > About FreeCAD > Copy to clipboard`; 
- Restart FreeCAD in safe mode `Help > Restart in safe mode` and try to reproduce the issue again. If the issue is resolved it can be fixed by deleting the FreeCAD config files.
- Start recording a macro `Macro > Macro recording...` and repeat all steps. Stop recording after the issue occurs and upload the saved macro or copy the macro code in the issue; 
- Post a Step-By-Step explanation on how to recreate the issue; 
- Upload an example file (FCStd as ZIP file) to demonstrate the problem; 

For more details see:

- [Bug Tracker](https://github.com/FreeCAD/FreeCAD/issues)
- [Reporting Issues and Requesting Features](https://github.com/FreeCAD/FreeCAD/issues/new/choose)
- [Contributing](https://github.com/FreeCAD/FreeCAD/blob/main/CONTRIBUTING.md)
- [Help Forum](https://forum.freecad.org/viewforum.php?f=3)
- [Developers Handbook](https://freecad.github.io/DevelopersHandbook/)

> [!NOTE]
The [FPA](https://fpa.freecad.org) offers developers the opportunity
to apply for a grant to work on projects of their choosing. Check
[jobs and funding](https://blog.freecad.org/jobs/) to know more.


Usage & Getting Help
--------------------

The FreeCAD wiki contains documentation on 
general FreeCAD usage, Python scripting, and development.
View these pages for more information:

- [Getting started](https://wiki.freecad.org/Getting_started)
- [Features list](https://wiki.freecad.org/Feature_list)
- [Frequent questions](https://wiki.freecad.org/FAQ/en)
- [Workbenches](https://wiki.freecad.org/Workbenches)
- [Scripting](https://wiki.freecad.org/Power_users_hub)
- [Development](https://wiki.freecad.org/Developer_hub)

The [FreeCAD forum](https://forum.freecad.org) is a great place
to find help and solve specific problems when learning to use FreeCAD.

---

<p>This project receives generous infrastructure support from
  <a href="https://www.digitalocean.com/">
    <img src="https://opensource.nyc3.cdn.digitaloceanspaces.com/attribution/assets/SVG/DO_Logo_horizontal_blue.svg" width="91px">
  </a> and <a href="https://www.kipro-pcb.com/">KiCad Services Corp.</a>
</p>
