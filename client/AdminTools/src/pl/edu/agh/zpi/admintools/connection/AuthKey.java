package pl.edu.agh.zpi.admintools.connection;

public class AuthKey implements IPacket {
	private byte[] key = new byte[16];
	
	public AuthKey() {
	}
	
	public AuthKey(byte[] key) {
		if(key.length == 16){
			this.key = key;
		}
	}
	
	public byte[] getKey() {
		return key;
	}
	
	public void setKey(byte[] key) {
		if(key.length == 16)
			this.key = key;
	}
	
	public byte[] toByteArray(){
		return key;
	}
}
