# ToastLib

A C++ (with C compatible interface) library for popping Toasts on Windows 10+.
Finally, your programs can be as annoying as your phone.

## Build

Set up MSVC environment and invoke build.bat

## How?

See examples.

## Powershell?

What?

```
function Get-Utf8Ptr {
	param ([string]$str)

	$bytes = [System.Text.Encoding]::UTF8.GetBytes($str + [char]0)
	$ptr = [System.Runtime.InteropServices.Marshal]::AllocHGlobal($bytes.Length)
	[System.Runtime.InteropServices.Marshal]::Copy($bytes, 0, $ptr, $bytes.Length)
	return $ptr
}

$source = @"
using System;
using System.Runtime.InteropServices;

public static class NativeMethods {
	[DllImport("C:\\example\\toastlib.dll", CallingConvention = CallingConvention.Cdecl)]
	public static extern int toast_show_simple(IntPtr title, IntPtr message, IntPtr app_id);
}
"@

Add-Type -TypeDefinition $source

$titlePtr = Get-Utf8Ptr "🧪 UTF-8 Title"
$messagePtr = Get-Utf8Ptr "Foobar! 🔥"
$appIdPtr = Get-Utf8Ptr "foo.bar.baz"

try {
	$result = [NativeMethods]::toast_show_simple($titlePtr, $messagePtr, $appIdPtr)
	Write-Host "Function returned: $result"
}
finally {
	[System.Runtime.InteropServices.Marshal]::FreeHGlobal($titlePtr)
	[System.Runtime.InteropServices.Marshal]::FreeHGlobal($messagePtr)
	[System.Runtime.InteropServices.Marshal]::FreeHGlobal($appIdPtr)
}
```

## License

MIT.
