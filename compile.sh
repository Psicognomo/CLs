
echo ' '
echo '### Starting compilation ... '

# Comincia ad inizializzare tutti i path per la compilazione
export WorkingDirectory=$(pwd)
export PackageDirectory=${WorkingDirectory}'/pkgs'
echo '# Setting working directory: '${WorkingDirectory}
echo '# Setting package directory: '${PackageDirectory}

# controlla che pkgs ci sia !
if [ ! -d ${PackageDirectory} ]; then
    echo '    -- ERROR : Package Directory not found !'
    echo '       Path  : '${PackageDirectory}
    echo '# Exiting with return code 0'
    echo ' '
    return 0
fi


export SrcDirectory=${WorkingDirectory}'/src'
echo '# Setting  source directory: '${SrcDirectory}
# controlla che src ci sia !
if [ ! -d ${SrcDirectory} ]; then
    echo '    -- ERROR : Source Directory not found !'
    echo '       Path  : '${SrcDirectory}
    echo '# Exiting with return code 0'
    echo ' '
    return 0
fi


# controlla che exe ci sia! Altrimenti crealo !!!
export ExeDirectory=${WorkingDirectory}'/exe'
echo '# Setting     exe directory: '${ExeDirectory}

if [ ! -d ${ExeDirectory} ]; then
    echo '    -- Creating exe directory ... '
    mkdir ${ExeDirectory}
else
    rm ${ExeDirectory}/*
    echo '    -- Cleaning exe directory ... '
fi


# Inizia a lavorare sui vari componenti
echo '# Setting packages ...'
export nPkgs=`find "${PackageDirectory}/" -mindepth 1 -maxdepth 1 -type d | wc -l`
echo '  Total of '${nPkgs}' package(s):'

export indexPkgs=0
for dirs in ${PackageDirectory}/* ; do
    PackagePath[ ${indexPkgs} ]=${dirs}'/trunk'
    echo '    -- Package Found: '${dirs#${PackageDirectory}/}

    # Controlla che il trunk ci sia
    if [ ! -d ${PackagePath[ ${indexPkgs} ]} ]; then
	echo '       ERROR : Trunk directory not found in the package !'
	echo '       Path  : '${PackagePath[ ${indexPkgs} ]}
	echo '# Exiting with return code 0'
	echo ' '

# Ripulisci l'array dei path per future compilazioni
    unset PackagePath

	return 0
    fi

    echo '       Path: '${PackagePath[ ${indexPkgs} ]}
    let "indexPkgs++"
done

indexPkgs=0
for f in ${PackagePath[@]}; do
    INC[ ${indexPkgs} ]='-I'${f}
    let "indexPkgs++"
done

# Crea tutti i files .o nella cartella exe
echo '# Compiling .o files in exe directory...'
for i in ${PackagePath[@]} ; do
    for f in ${i}/*.cxx ; do
	export cxxFile=${f#${i}'/'}
	echo '    -- compiling '${cxxFile%'.cxx'}'.o'
	g++ ${INC[@]} -c ${f} -o ${ExeDirectory}/${cxxFile%'.cxx'}.o `root-config --cflags` 
#	g++ ${INC[@]} -std=c++11 -c ${f} -o ${ExeDirectory}/${cxxFile%'.cxx'}.o `root-config --cflags` 

#check if the command succeded
	if [ ! $? -eq 0 ]; then
	    echo '       ERROR : creation of the .o file did not complete successfully!'
	    echo '# Exiting with return code 0'
	    echo ' '

# Ripulisci l'array dei path per future compilazioni
	    unset PackagePath
	    unset INC

	    return 0
	fi

    done
done


echo '# Compiling ...'

# Compilazione
g++ ${INC[@]} ${SrcDirectory}/main.cxx -o main ${ExeDirectory}/*.o `root-config --glibs --cflags` -lRooFit -lRooFitCore
#g++ ${INC[@]} -std=c++11 ${SrcDirectory}/main.cxx -o main ${ExeDirectory}/*.o `root-config --glibs --cflags` -lRooFit -lRooFitCore

#check if the command succeded
if [ ! $? -eq 0 ]; then
    echo '       ERROR : compilation did not complete successfully!'
    echo '# Exiting with return code 0'
    echo ' '

# Ripulisci l'array dei path per future compilazioni
    unset PackagePath
    unset INC

    return 0
fi

# Ripulisci l'array dei path per future compilazioni
unset PackagePath
unset INC

echo '### Compilation : DONE'
echo ' '


