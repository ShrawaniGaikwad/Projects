import java.sql.*;
public class DB {

    public static void main(String Arg[])
    {
        try
        {
            String URL="jdbc:mysql://localhost:3306/ppa49";
            String username="root";
            String password = "root";
            String Query ="select * from student";

            // Class.forName("com.mysql.jdbc.Driver");
            Connection cobj=DriverManager.getConnection(URL,username,password);

            Statement sobj=cobj.createStatement();
            ResultSet robj=sobj.executeQuery(Query);

            while(robj.next())
        {
            System.out.println("RID : "+robj.getInt("RNO"));
            System.out.println("Name : "+robj.getString("name"));
            System.out.println("Address : "+robj.getString("Address"));
            System.out.println("Marks : "+robj.getInt("Marks"));
        }
        }
        catch(Exception eobj)
        {
            System.out.println("Exception occured"+eobj);
        }
    }

}
