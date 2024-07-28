import java.util.*;
import java.io.*;

class program25
{
    public static void main(String argc[]) throws Exception
    {
        System.out.println("---------------------------------------------------");
        System.out.println("-------------Marvellous Packer Unpacker------------");
        System.out.println("---------------------------------------------------");
        int FileSize=0;
        int iCnt=0;
        String str=null;
        int iRet=0;
        Scanner sobj = new Scanner(System.in);

        System.out.println("Enter the name of packed file that you want to unpack");
        String PackedFile=sobj.nextLine();
        File fobj = new File(PackedFile);

        FileInputStream fiobj = new FileInputStream(fobj);
        byte Header[]=new byte[100];

        while((iRet=fiobj.read(Header,0,100))!=-1)
        {
            String Hstr=new String(Header);
            str=Hstr.trim();
            String Tokens[]=str.split(" ");

            File NewFile=new File(Tokens[0]);
            NewFile.createNewFile();

            FileSize=Integer.parseInt(Tokens[1]);
            byte Buffer[]=new byte[FileSize];

            fiobj.read(Buffer,0,FileSize);
            FileOutputStream foobj=new FileOutputStream(NewFile);
            foobj.write(Buffer,0,FileSize);
            System.out.println("The file Unpacked successfully is:"+Tokens[0]);
            iCnt++;
        }
        System.out.println("Total Unpacked Files are:"+iCnt);
        System.out.println("Unpacking activity completed");
        System.out.println("--------------------------------------------------------------------------");
        System.out.println("------------Thank you for using Marvellous Packer Unpacker ---------------");
        System.out.println("---------------------------------------------------------------------------");

    }
}