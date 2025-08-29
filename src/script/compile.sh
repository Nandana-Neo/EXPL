cd ..

input="inp.expl"
temp="temp.xsm"
output="out.xsm"

while [[ $# -gt 0 ]]; do
    case $1 in
        -i|--input)
            input="$2"
            shift 2
            ;;
        -o|--output)
            output="$2"
            shift 2
            ;;
        -t|--temp)
            temp="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            shift
            ;;
    esac
done

./output -i $input -o $temp
./label_translator -i $temp -o $output
# rm temp.xsm
# Run `./xsm -l library.lib -e src/output` in console in parent folder xsm_expl