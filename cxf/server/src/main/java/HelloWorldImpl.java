import javax.jws.WebService;
 
@WebService(endpointInterface = "HelloWorld")
public class HelloWorldImpl implements HelloWorld {
 
    public String sayHi(String text) {
        System.out.println("sayHi called");
        return "Hello " + text;
    }
}