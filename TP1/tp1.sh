# Punto a b y c 
sed 's/\./\.\n/g' breve_historia.txt | sed '/^\s*$/d' > breve_historia_2.txt

# Punto d
grep -i '\sguerra\b' breve_historia_2.txt

# Punto e
egrep '^A.*s\.?$' breve_historia_2.txt

# Punto f
grep -c '\speronismo\b' breve_historia_2.txt

# Punto g
grep -c '\sSarmiento\b.*\sRosas\b\|\sRosas\b.*\sSarmiento\b' breve_historia_2.txt

# Punto h
egrep '\s18[0-9]{2}\b|\sXIX\b' breve_historia_2.txt

# Punto i
sed 's/^\s*[a-zA-Zá-ú]*\b//g' breve_historia_2.txt

# Punto j
ls | grep '.*\.txt'