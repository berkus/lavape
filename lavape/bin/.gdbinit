# This file defines handy gdb macros for printing out Qt types
# To use it, add this line to your ~/.gdbinit :
# source /path/to/kde/sources/kdesdk/scripts/kde-devel-gdb

# Please don't use tabs in this file. When pasting a
# macro definition to gdb, tabs are interpreted as completion.

# Disable printing of static members. Qt has too many, it clutters the output
#set print static-members off
set print pretty on
set print object on
set print vtbl on

define qs
    printqstringdata $arg0.d
end
document qs
  Prints the contents of a QString
end

define printqstringdata
    set $i=0
    set $d = (QStringData *)$arg0
    while $i < $d->len
        printf "%c", (char)($d->unicode[$i++].ucs & 0xff)
    end
    printf "\n"
end
document printqstringdata
  Prints the contents of a QStringData
  This is useful when the output of another command (e.g. printqmap)
  shows {d = 0xdeadbeef} for a QString, i.e. the qstringdata address
  instead of the QString object itself.
  printqstring $s and printqstringdata $s.d are equivalent.
end

define printqstring_utf8
   set $i=0
   set $s = $arg0
   while $i < $s.d->len
     set $uc = (unsigned short) $s.d->unicode[$i++].ucs
     if ( $uc < 0x80 )
       printf "%c", (unsigned char)($uc & 0x7f)
     else
       if ( $uc < 0x0800 )
         printf "%c", (unsigned char)(0xc0 | ($uc >> 6))
       else
         printf "%c", (unsigned char)(0xe0 | ($uc >> 12)
         printf "%c", (unsigned char)(0x80 | (($uc > 6) &0x3f)
       end
       printf "%c", (unsigned char)(0x80 | ((uchar) $uc & 0x3f))
     end
   end
   printf "\n"
end
document printqstring_utf8
  Prints the contents of a QString encoded in utf8. 
  Nice if you run your debug session in a utf8 enabled terminal.
end

define printqcstring
    print $arg0.shd.data
    print $arg0.shd.len
end
document printqcstring
  Prints the contents of a QCString (char * data, then length)
end

define printqfont
    print *($arg0).d
    printqstring ($arg0).d->request.family
    print ($arg0).d->request.pointSize
end
document printqfont
  Prints the main attributes from a QFont, in particular the requested
  family and point size
end

define printqcolor
    printf "(%d,%d,%d)\n", ($arg0).red(), ($arg0).green(), ($arg0).blue()
end
document printqcolor
  Prints a QColor as (R,G,B).
  Usage: 'printqcolor <QColor col>
end

define printqptrvector
    # Maybe we could find it out the type by parsing "whatis $arg0"?
    set $arr = $arg0
    set $len = $arr->len
    set $i = 0
    while $i < $len
       # print "%s[%d] = %s\n", $arr, $i, *($arg1 *)(($arr->vec)[$i])
       print *($arg1 *)(($arr->vec)[$i])
       $i++
    end
end
document printqptrvector
  Prints the contents of a QPtrVector. Pass the type as second argument.
end

define printqptrvectoritem
    set $arr = $arg0
    set $i = $arg2
    print ($arg1 *)(($arr->vec)[$i])
    print *($arg1 *)(($arr->vec)[$i])
end
document printqptrvectoritem
  Print one item of a QPtrVector
  Usage: printqptrvectoritem vector type index
end

define printqmap
    set $map = $arg0
    set $len = $map.sh->node_count
    output $len
    printf " items in the map\n"
    set $header = $map.sh->header
    # How to parse the key and value types from whatis?
    set $it = (QMapNode<$arg1,$arg2> *)($header->left)
    while $it != $header
        printf " key="
        output $it->key
        printf " value="
        output $it->data
        printf "\n"
        _qmapiterator_inc $it
        set $it = (QMapNode<$arg1,$arg2> *)($ret)
    end
end
document printqmap
  Prints the full contents of a QMap
  Usage: 'printqmap map keytype valuetype'
end


define _qmapiterator_inc
    set $ret = $arg0
    if $ret->right != 0
        set $ret = $ret->right
        while $ret->left != 0
            set $ret = $ret->left
        end
    else
        set $y = $ret->parent
        while $ret == $y->right
            set $ret = $y
            set $y = $y->parent
        end
        if $ret->right != $y
            set $ret = $y
        end
    end
end
document _qmapiterator_inc
  Increment a qmap iterator (internal method, used by printqmap)
end

define printqvaluelist
    set $list = $arg0
    set $len = $list.sh->nodes
    output $len
    printf " items in the list\n"
    set $it = $list.sh->node->next
    set $end = $list.sh->node
    while $it != $end
        output $it->data
        printf "\n"
        set $it = $it->next
    end
end
document printqvaluelist
  Prints the contents of a QValueList.
  Usage: printqvaluelist mylist
end

define printqstringlist
    set $list = $arg0
    set $len = $list.sh->nodes
    output $len
    printf " items in the list\n"
    set $it = $list.sh->node->next
    set $end = $list.sh->node
    while $it != $end
        printqstring $it->data
        set $it = $it->next
    end
end
document printqstringlist
  Prints the contents of a QStringList.
  Usage: printqstringlist mylist
end

