New-Item -ItemType Directory -Force `
    B:\BuildCache\sccache, `
    B:\BuildCache\vcpkg | Out-Null

[Environment]::SetEnvironmentVariable(
    "SCCACHE_DIR",
    "L:\\BuildCache\\sccache",
    "User"
)

[Environment]::SetEnvironmentVariable(
    "SCCACHE_CACHE_SIZE",
    "40G",
    "User"
)

[Environment]::SetEnvironmentVariable(
    "VCPKG_BINARY_SOURCES",
    "clear;files,B:\\BuildCache\vcpkg,readwrite",
    "User"
)