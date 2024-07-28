import java.util.*;
import java.io.*;

class program6
{
    public static void main(String args[]) throws Exception
    {
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
            for(int i=0;i<Arr.length;i++)
            {
            
                Header=Arr[i].getName()+" "+Arr[i].length();
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
                System.out.println(Arr[i]);

                //write data into packed file
                while((iRet=fiobj.read(Buffer))!=-1)
                {
                    fcombine.write(Buffer,0,iRet);
                }
                fiobj.close();
            }
        }
        else
        {
            System.out.println("Directory is not present");
        }

    }
}