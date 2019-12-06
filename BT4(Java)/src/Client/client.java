package Client;

import java.net.*;
import java.io.*;
import java.util.*;

public class client {

    private static Socket sock;
    private static String fileName;
    private static BufferedReader stdin;
    private static PrintStream os;
    private static String ip;

    public static void main(String[] args) throws IOException {
        System.out.println("Nhập địa chỉ IP: ");
        Scanner scanner = new Scanner(System.in);
        ip = scanner.nextLine();
        while(true && ip!=null) {
            try {
                sock = new Socket(ip, 25444);
                stdin = new BufferedReader(new InputStreamReader(System.in));
            } catch (Exception e) {
                System.err.println("Không thể kết nối server.");
                System.exit(1);
            }

            os = new PrintStream(sock.getOutputStream());

            try {
                switch (Integer.parseInt(selectAction())) {
                    case 1:
                        os.println("1");
                        File fol = new File("src/Server/SharedFolder/");
                        if (fol.isDirectory()) {
                            final File[] children = fol.listFiles();
                            if (children == null) {
                                return;
                            }
                            else{
                                for (final File each : children) {
                                    System.out.println(each.getName());
                                }
                            }
                        }
                        continue;
                    case 2:
                        os.println("2");
                        System.out.print("Nhập tên file muốn tải xuống: ");
                        fileName = stdin.readLine();
                        os.println(fileName);
                        receiveFile(fileName);
                        continue;
                    case 3:
                        os.println("3");
                        System.out.print("Nhập tên file muốn up lên server: ");
                        fileName = stdin.readLine();
                        os.println(fileName);
                        sendFile(fileName);
                        continue;
                    case 4:
                        sock.close();
                        System.exit(1);
                }
            } catch (Exception e) {
                System.err.println(e.getMessage());
            }

        }

    }

    public static String selectAction() throws IOException {
        System.out.println("1. List files.");
        System.out.println("2. Download file.");
        System.out.println("3. Upload file..");
        System.out.println("4. Logout.");
        System.out.print("\nChọn lệnh: ");

        return stdin.readLine();
    }

    public static void receiveFile(String fileName) {
        try {
            int bytesRead;
            InputStream in = sock.getInputStream();

            DataInputStream clientData = new DataInputStream(in);

            fileName = clientData.readUTF();
            OutputStream output = new FileOutputStream("src/Client/"+fileName);
            long size = clientData.readLong();
            byte[] buffer = new byte[1024];
            while (size > 0 && (bytesRead = clientData.read(buffer, 0, (int) Math.min(buffer.length, size))) != -1) {
                output.write(buffer, 0, bytesRead);
                size -= bytesRead;
            }

            output.close();
            in.close();

            System.out.println("File "+fileName+" received from Server.");
        } catch (IOException ex) {
            System.out.println("Exception: "+ex);
        }

    }
public static void sendFile(String file) throws IOException {
    ObjectInputStream ois = new ObjectInputStream(sock.getInputStream());
    ObjectOutputStream oos = new ObjectOutputStream(sock.getOutputStream());
    File myFile= new File("src/Client/SharedFolder/"+file);
    oos.writeObject(file);
    FileInputStream fis = new FileInputStream(myFile);
    byte [] buffer = new byte[4096];
    Integer bytesRead = 0;
    while ((bytesRead = fis.read(buffer)) > 0) {
        oos.writeObject(bytesRead);
        oos.writeObject(Arrays.copyOf(buffer, buffer.length));
    }
    System.out.println("File "+fileName+" saved to Server.");
}
}