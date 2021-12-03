# .NetCoreApp examples
The following examples showcase how to use OrTools.
The project solution has examples for both C# and F#.

We recommend that all projects you create target `netcoreapp2.1`,
as this allows you to compile for various frameworks and
keep up-to-date with the latest frameworks.

Wherever you have or-tools installed, be sure to `PackageReference` the `Google.OrTools`
from the project file.

# Execution
Running the examples will involve building them, then running them.   
You can run the following command:
```shell
dotnet build <example>.csproj
dotnet run --no-build --project <example>.csproj
```

# Note on Google.OrTools.FSharp
This part describes how to use Google.OrTools.FSharp nuget package in F#.

## SolverOptions and lpSolve
This function and parameter object are a wrapper around the standard Google.OrTools functions.  
It is designed to enter the Linear/Integer program as *matrices* and *vectors*.  

Two input formats are allowed:
* Canonical Form;
* Standard Form.

**ALL Matrices & Vectors are entered as columns**

## Execution
Running the examples will involve building them, then running them.   
You can run the following command:
```shell
dotnet build <example>.fsproj
dotnet run --no-build --project <example>.fsproj
```

