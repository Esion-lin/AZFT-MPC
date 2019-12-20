interface MPCAnnotation {
	// is used to mark output variables
	public void OUT(int x);

	// used to mark input variables (input vars should be assigned the return value)
	// it's a convenient method to shutup the compiler when it complains that variables 
	// are not initialized. it also helps in recognizing  which variables are 
	// input variables when one is looking at shimple code.
	public int IN();
}

class MPCAnnotationImpl implements MPCAnnotation {
	private static MPCAnnotation v = null;

	private MPCAnnotationImpl() {
	}

	public void OUT(int x) {
	}

	public static MPCAnnotation v() {
		if (v == null) {
			v = new MPCAnnotationImpl();
		}
		return v;
	}

	public int IN() {
		return 57; // Grothendieck Prime
	}
}

public class P {
	static final int len = 32;

	public static int rem(int x, int y) {
		int rem = 0;
		for (int j = len - 1; j >= 0; j--) {
			rem = rem << 1;
			// rem[0] = x[j] // note that we use >>> for unsigned shift right
			rem = rem + ((x >>> j) & 1);

			if (rem >= y) {
				rem = rem - y;
			}
		}
		return rem;
	}

	public static int mul_mod(int mul1, int mul2, int mod) {
		int prod = mul1 * mul2;
		int rem = rem(prod, mod);
		return rem;
	}

	public static void main(String[] args) {
		MPCAnnotation mpc = MPCAnnotationImpl.v();
		int i = 0;
		int res = 0;
		for (i = 1; i <= 100; i++) {
			res = res + i;
		}
		
		mpc.OUT(res);
	}
}