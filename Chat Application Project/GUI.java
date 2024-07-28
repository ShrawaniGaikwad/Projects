import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;

class GUI {
    public static void main(String[] args) throws Exception {
        SwingUtilities.invokeLater(() -> {
            try {
                Logic lobj = new Logic();
            } catch (Exception eobj) {
                System.out.println(eobj);
            }
        });
    }
}

class Logic extends Frame implements ActionListener, WindowListener {
    public Frame fobj;
    public Label l1, l2, l3,l4;
    public TextArea messageArea, messageArea2;
    public TextField tobj1;
    public Button sendButton;
    private PrintStream ps;

    public Logic() throws Exception {
        // Create the main frame
        fobj = new Frame("Chat Application");
        fobj.setLayout(null);
        fobj.addWindowListener(this);
        fobj.setSize(1600, 900);
        fobj.setBackground(Color.PINK);
        fobj.setVisible(true);

        // Create a panel with a border to hold all components
        JPanel panel = new JPanel();
        panel.setLayout(null);
        panel.setBounds(400, 40, 800, 750);
        panel.setBackground(Color.LIGHT_GRAY);
        panel.setBorder(BorderFactory.createLineBorder(Color.BLACK, 2));
        fobj.add(panel);

        // Create and add labels to the panel
        l1 = new Label("Welcome to the Chat Application!");
        l1.setBounds(250, 40, 400, 40);
        l1.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l1);

        l2 = new Label("Server application is running");
        l2.setBounds(250, 100, 400, 40);
        l2.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l2);

        l3 = new Label("Awaiting client connection");
        l3.setBounds(250, 160, 400, 40);
        l3.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l3);

        l4 = new Label("");
        l4.setBounds(250, 160, 400, 40);
        l4.setFont(new Font("Tahoma", Font.BOLD, 24));
        panel.add(l4);

        // Create and add a text area for received messages
        messageArea = new TextArea();
        messageArea.setBounds(50, 220, 700, 200);
        messageArea.setFont(new Font("Tahoma", Font.PLAIN, 18));
        messageArea.setEditable(false);
        panel.add(messageArea);

        // Create and add a text area for sent messages
        messageArea2 = new TextArea();
        messageArea2.setBounds(50, 440, 700, 200);
        messageArea2.setFont(new Font("Tahoma", Font.PLAIN, 18));
        messageArea2.setEditable(false);
        panel.add(messageArea2);

        // Create and add text field and send button
        tobj1 = new TextField();
        tobj1.setBounds(50, 660, 500, 40);
        panel.add(tobj1);

        sendButton = new Button("Send");
        sendButton.setBackground(Color.GREEN);
        sendButton.setBounds(560, 660, 100, 40);
        sendButton.addActionListener(this);
        panel.add(sendButton);

        // Start server in a separate thread
        Thread serverThread = new Thread(this::runServer);
        serverThread.start();
    }

    private void runServer() {
        String s1;
        try {
            ServerSocket ss = new ServerSocket(1100);
            SwingUtilities.invokeLater(() -> l3.setText("Waiting for client connection..."));

            Socket s = ss.accept();
            SwingUtilities.invokeLater(() -> l3.setText("Client connected"));

            BufferedReader br = new BufferedReader(new InputStreamReader(s.getInputStream()));
            ps = new PrintStream(s.getOutputStream());

            while ((s1 = br.readLine()) != null) {
                final String clientMessage = s1;
                SwingUtilities.invokeLater(() -> {
                    System.out.println("Client Says: " + clientMessage);
                    messageArea.append("Client Says: " + clientMessage + "\n");
                });

                // Wait for the user to input a message and press send
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }

            s.close();
            ss.close();
            br.close();
            ps.close();
        } catch (Exception eobj) {
            System.out.println(eobj);
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == sendButton) {
            l4.setText("Type Your Message : ");
            String s3 = tobj1.getText();
            ps.println(s3);
            messageArea2.append("Sent: " + s3 + "\n");
            tobj1.setText("");

            // Notify the server thread that a message has been sent
            synchronized (this) {
                this.notify();
            }
        }
    }

    // Implementing WindowListener methods (not all methods are used)
    public void windowClosing(WindowEvent wobj) {
        System.exit(0);
    }

    public void windowOpened(WindowEvent wobj) {}

    public void windowActivated(WindowEvent wobj) {}

    public void windowDeactivated(WindowEvent wobj) {}

    public void windowIconified(WindowEvent wobj) {}

    public void windowDeiconified(WindowEvent wobj) {}

    public void windowClosed(WindowEvent wobj) {}
}
