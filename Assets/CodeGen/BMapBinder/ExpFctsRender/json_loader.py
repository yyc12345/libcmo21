import json
import typing
import utils
from dataclasses import dataclass


class VariableType:
    """The class represent the type of each parameters and function return value."""

    __base_type_hierarchy: list[str]
    """
    The base type of this variable removing all ending stars (remove all pointer levels).
    Each item in this list is a part of namespace and the last one must be the type itself
    (without any namespace constraint).
    If no namespace constraint for this type, this list will only have one item.

    For end user, it is enough that knowing the last item is type itself.
    """
    __pointer_level: int
    """
    The pointer level of this type. 
    It is equal to the count of trailing star of this field in C style representation.
    """

    def __init__(
        self, base_type_hierarchy: list[str] = [], pointer_level: int = 0
    ) -> None:
        """Construct a new varible type."""
        self.__base_type_hierarchy = base_type_hierarchy
        self.__pointer_level = pointer_level

    def clone(self) -> "VariableType":
        """CLone self into a new instance."""
        return VariableType(list(self.__base_type_hierarchy), self.__pointer_level)

    @staticmethod
    def from_c_type(ctype: str) -> "VariableType":
        """
        Set this variable type with a type string in C/C++ style.

        For example, "NSTest::NSTest2::MyType**" will produce 2 pointer level
        with ('NSTest', 'NSTest2', 'MyType') as its base type hierarchy.

        :param ctype: The type string in C/C++ style.
        :return: The parsed VariableType instance.
        """
        if len(ctype) == 0:
            raise RuntimeError("empty string can not be parsed")

        # get pointer part and name part
        length = len(ctype)
        star_index = ctype.find("*")
        name_part: str
        pointer_level: int
        if star_index == -1:
            # No star, no pointer level
            name_part = ctype
            pointer_level = 0
        else:
            # Has star
            if star_index == 0:
                raise RuntimeError("base type not found")
            name_part = ctype[0:star_index]
            pointer_level = length - star_index

        # resolve name part
        base_type_hierarchy = list(name_part.split("::"))

        # return value
        return VariableType(base_type_hierarchy, pointer_level)

    def to_c_type(self) -> str:
        """
        Build a type string represented by this variable type in C/C++ style.

        :return: The type string in C/C++ style.
        """
        return "::".join(self.__base_type_hierarchy) + ("*" * self.__pointer_level)

    def get_base_type(self) -> str:
        """
        Get the base type of this variable type without any namespace.

        It just simply get the last entry in type hierarchy.

        :return: The base type string without namespace prefix.
        """
        return self.__base_type_hierarchy[-1]

    def is_pointer(self) -> bool:
        """
        Check whether this variable type is a pointer.
        This function just check whether the pointer level of this variavle type is zero.

        :return: True if it is pointer, otherwise false.
        """
        return self.__pointer_level != 0

    def get_pointer_level(self) -> int:
        """
        Return the pointer level of this variable type.

        You can simply assume the pointer level is equal to the count of trailing star.

        :return: The pointer level integer. Zero means that this type is not a pointer.
        """
        return self.__pointer_level

    def iter_base_type_hierarchy(self) -> typing.Iterator[str]:
        """
        Return the clone of the type hierarchy of this variable type.

        It is rarely used.
        This only should be used when you need the namespace hierarchy of this variable type.

        :return: The clone of current variable type hierarchy.
        """
        return iter(self.__base_type_hierarchy)

    # def is_valid(self) -> bool:
    #     """
    #     Check whether this type is a valid one.

    #     It actually check whether type hierarchy include at least one entry.

    #     :return: True if no problem of this type, otherwise false.
    #     """
    #     return len(self.__base_type_hierarchy) != 0

    def get_pointer_of_this(self) -> "VariableType":
        """
        Return a new created variable type which is the pointer of this variable type.

        In internal implementation, it just create a clone of current variable type
        with the increase of pointer level by 1.

        :return: The new created pointer type of this variable type.
        """
        return VariableType(list(self.__base_type_hierarchy), self.__pointer_level + 1)

    @staticmethod
    def from_json(data: str) -> "VariableType":
        return VariableType.from_c_type(data)


@dataclass(frozen=True)
class ExpFctParam:
    """The class represent a single parameter (argument) of function."""

    var_type: VariableType
    """The type of this parameter."""
    var_name: str
    """The name of this parameter."""
    is_input: bool
    """
    True if this parameter is marked as input parameter, otherwise false.
    
    Input parameter and output parameter is commonly used in C/C++ code.
    By using this feature, each function can receive multiple arguments 
    and return multiple arguments without defining a struct to hold it.
    
    The type of input parameter is itself. 
    However, the type of output parameter is the pointer of itself. 
    So you may need get its pointer type when processing output parameter, 
    especially for the scenario that the target language do not support explicit output parameter keyword.
    """
    var_desc: str
    """
    The description of this parameter.

    This description is generated by this program.
    It will indicate the underlying C++ type to tell end user how to treat this parameter 
    because some target languages' native calling style can not represent these detail.

    In this program, this field must be written as a docstring of corresponding function.
    """

    @staticmethod
    def from_json(data: dict[str, typing.Any]) -> "ExpFctParam":
        return ExpFctParam(
            VariableType.from_c_type(data["type"]),
            data["name"],
            data["is_input"],
            data["desc"],
        )


@dataclass(frozen=True)
class ExpFct:
    """The class represent an export BMap function."""

    fct_name: str
    """The name of this function."""
    fct_rv_type: VariableType
    """The return value type of this function."""
    fct_params: list[ExpFctParam]
    """
    The parameters (arguments) list of this function.
    Each item represent parameter accepted by this function one by one from left to right.
    """

    @staticmethod
    def from_json(data: dict[str, typing.Any]) -> "ExpFct":
        return ExpFct(
            data["name"],
            VariableType.from_json(data["return"]),
            list(map(lambda i: ExpFctParam.from_json(i), data["params"])),
        )


@dataclass(frozen=True)
class ExpFctCollection:
    """The class represent a collection of export BMap functions."""

    fcts: list[ExpFct]
    """The collection of exported BMap functions."""

    @staticmethod
    def from_json(data: list[typing.Any]) -> "ExpFctCollection":
        return ExpFctCollection(list(map(lambda i: ExpFct.from_json(i), data)))


def load_fcts(filename: str) -> ExpFctCollection:
    with open(utils.get_input_file_path(filename), "r", encoding="utf-8") as f:
        return ExpFctCollection.from_json(json.load(f))
