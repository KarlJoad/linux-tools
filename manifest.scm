(use-modules (gnu packages base)
             (gnu packages commencement)
             (gnu packages gdb)
             (gnu packages man))

(packages->manifest
 (list gcc-toolchain gnu-make
       gdb
       man-db man-pages))
