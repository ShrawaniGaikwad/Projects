import java.util.*;
import java.io.*;

class program17
{
    public static void main(String args[]) throws Exception
    {
        System.out.println("---------------------------------------------------");
        System.out.println("-------------Marvellous Packer Unpacker------------");
        System.out.println("---------------------------------------------------");
       Scanner sobj = new Scanner(System.in);
       System.out.println("Enter the name of the Directory");
       String Directory=sobj.nextLine();

       System.out.println("Enter the name of packed file that you want to create");
       String PackedFile = sobj.nextLine();

       boolean bRet=false;
       File fobjPack=new File(PackedFile);
       bRet=fobjPack.createNewFile();
       if(bRet==false)
       {
        System.out.println("Unable to create packed file..");
        return;
       }

        System.out.println("Packed file gets successfully created");

        File fobj = new File(Directory);

        bRet=fobj.isDirectory();
        if(bRet==true)
        {
            System.out.println("Directory is present");

            File Arr[]=fobj.listFiles();  //Array of files
            System.out.println("Number of files in the directory are:"+Arr.length);
            String Header=null;
            FileOutputStream fcombine =new FileOutputStream(PackedFile);
            int iRet=0;
            byte Buffer[]=new byte[1024];
            System.out.println("Packing activity started");

            for(int i=0;i<Arr.length;i++)
            {
            
                Header="/"+Arr[i].getName()+" "+Arr[i].length();

                //Add extra spaces
                for(int j=Header.length();j<100;j++)
                {
                    Header=Header+" ";
                }
                //convert string header to byte
                byte hArr[]=Header.getBytes();

                //write header into packed file
                fcombine.write(hArr,0,100);

                FileInputStream fiobj =new FileInputStream(Arr[i]);

                //write data into packed file
                while((iRet=fiobj.read(Buffer))!=-1)
                {
                    fcombine.write(Buffer,0,iRet);
                }
                System.out.println("File packed is:"+Arr[i].getName());
                fiobj.close();
            }
            System.out.println("Packing activity completed");
            System.out.println("Total files packed successfully:"+Arr.length);

            //--------------------------------------UNPACKING------------------------------------------------//
            System.out.println("Unpacking activity started");
            FileInputStream PackedF=new FileInputStream(PackedFile);
            byte header[]=new byte[100];
            while((iRet=PackedF.read(header,0,100))!=-1)
            {
                String hstr=new String(header);
                if(hstr.lastIndexOf("/")>=0)
                {
                    String ext = hstr.substring(hstr.lastIndexOf("/"));
                    ext=ext.substring(1);

                    String words[]=ext.split("\\s");  // \\s it is used to represent a white space character
                    String fileName=words[0];
                    int Size=Integer.parseInt(words[1]);
                    byte arr[]=new byte[Size];
                    PackedF.read(arr,0,Size);
                    System.out.println("File created:"+fileName);
                    FileOutputStream fout=new FileOutputStream(fileName);
                    fout.write(arr,0,Size); 
                }
            }
            System.out.println("Unpacking activity completed");
            System.out.println("--------------------------------------------------------------------------");
            System.out.println("------------Thank you for using Marvellous Packer Unpacker ---------------");
            System.out.println("---------------------------------------------------------------------------");

        }
        else
        {
            System.out.println("Directory is not present");
        }

    }
}