dd if=/dev/zero of=system.img bs=512 count=2880
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=system.img bs=512 count=2 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc  
dd if=args.img of=system.img bs=512 count=1 seek=512 conv=notrunc  

echo Compile kernel, mat, text, folderIO
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o mat.o library/mat/mat.c
bcc -ansi -c -o text.o library/text/text.c
bcc -ansi -c -o folderIO.o library/folderIO/folderIO.c
bcc -ansi -c -o folderIO2.o library/folderIO/folderIO2.c
bcc -ansi -c -o folderIO3.o library/folderIO/folderIO3.c
bcc -ansi -c -o fileIO.o library/fileIO/fileIO.c
bcc -ansi -c -o utility.o utility/utility.c
bcc -ansi -c -o system.o library/system/system.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o mat.o text.o folderIO.o folderIO2.o system.o 
dd if=kernel of=system.img bs=512 conv=notrunc seek=3
echo Compile kernel selesai

echo Compile shell
bcc -ansi -c -o shell.o shell.c
nasm -f as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o mat.o text.o fileIO.o folderIO2.o folderIO3.o system.o
echo Compile shell selesai

echo Compile mkdir
bcc -ansi -c -o mkdir.o utility/mkdir.c
ld86 -o mkdir -d mkdir.o lib_asm.o mat.o text.o fileIO.o folderIO2.o folderIO3.o system.o
echo Compile mkdir selesai

echo Compile cat
bcc -ansi -c -o cat.o utility/cat.c
ld86 -o cat -d cat.o lib_asm.o mat.o text.o fileIO.o folderIO3.o system.o
echo Compile cat selesai

echo Compile cp
bcc -ansi -c -o cp.o utility/cp.c
ld86 -o cp -d cp.o lib_asm.o mat.o text.o fileIO.o folderIO.o folderIO2.o folderIO3.o system.o
echo Compile cp selesai

echo Compile rm
bcc -ansi -c -o rm.o utility/rm.c
ld86 -o rm -d rm.o lib_asm.o mat.o text.o fileIO.o folderIO.o folderIO2.o folderIO3.o system.o
echo Compile rm selesai

echo Compile mv
bcc -ansi -c -o mv.o utility/mv.c
ld86 -o mv -d mv.o lib_asm.o mat.o text.o fileIO.o folderIO.o folderIO2.o folderIO3.o system.o
echo Compile mv selesai

echo Compile loadFile
gcc loadFile.c -o loadFile
echo Compile loadFile selesai

echo load files
echo load shell
./loadFile shell
echo load mkdir
./loadFile mkdir
echo load cat
./loadFile cat
echo load cp
./loadFile cp
echo load rm
./loadFile rm
echo load mv
./loadFile mv
echo load test.txt
./loadFile test.txt
echo load file selasai