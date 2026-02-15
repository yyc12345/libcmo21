import json
import typing
import utils
import enum


class BEnumEntryFlagKind(enum.StrEnum):
    """
    The kind of enum entry value.
    This kind indicates whether this enum entry belong to a flag enum.
    """

    NotFlag = "not-flag"
    """
    This enum entry can not belong to a flag enum.
    Because its value is ordinary.
    """
    MayFlag = "may-flag"
    """
    This enum entry may belong to a flag enum.
     Because its value is in HEX format, and refering other members.
    """
    MustFlag = "must-flag"
    """
    This enum entry must belong to a flag enum.
    Because its value use bitwise operation.
    """


class BEnumEntrySignKind(enum.StrEnum):
    """
    The kind of enum entry value.
    This kind indicates the sign of this enum entry value.
    """

    Positive = "positive"
    """The value of this enum entry is positive number or zero."""
    Negative = "negative"
    """he value of this enum entry is negative number."""
    Unknown = "unknown"
    """
    The value of this enum entry is unknown.
    This is may be caused by that it refer other memeber.
    """


class BEnumEntry:
    """The struct to describe the entry of an enum."""

    __entry_name: str
    """The name of this entry."""
    __entry_value: str | None
    """The value of this entry. None if this entry do not have explicit value."""
    __entry_flag_kind: BEnumEntryFlagKind
    """The flag kind of this entry value."""
    __entry_sign_kind: BEnumEntrySignKind
    """The sign kind of this entry value."""
    __entry_comment: str | None
    """The comment of this entry. None if no comment."""

    def __init__(
        self,
        entry_name: str,
        entry_value: str | None,
        entry_flag_kind: BEnumEntryFlagKind,
        entry_sign_kind: BEnumEntrySignKind,
        entry_comment: str | None,
    ):
        self.__entry_name = entry_name
        self.__entry_value = entry_value
        self.__entry_flag_kind = entry_flag_kind
        self.__entry_sign_kind = entry_sign_kind
        self.__entry_comment = entry_comment

    def get_entry_name(self) -> str:
        """Get the name of this entry."""
        return self.__entry_name

    def get_entry_value(self) -> str | None:
        """Get the value of this entry. None if this entry do not have explicit value."""
        return self.__entry_value

    def get_entry_comment(self) -> str | None:
        """Get the comment of this entry. None if no comment."""
        return self.__entry_comment

    @staticmethod
    def from_json(data: dict[str, typing.Any]) -> "BEnumEntry":
        return BEnumEntry(
            data["name"],
            data.get("value", None),
            BEnumEntryFlagKind(data.get("flag_kind")),
            BEnumEntrySignKind(data.get("sign_kind")),
            data.get("comment", None),
        )


class BHierarchyEnumEntry(BEnumEntry):
    """
    The specialized EnumEntry type which can store extra hierarchy info.
    Used in CK_CLASSID parsing.
    """

    __hierarchy: list[str]
    """
    The list to store this CK_CLASSID inheritance relationship.
    The first item is the oldest parent in inheritance.
    The last item is self.
    """

    def __init__(
        self,
        entry_name: str,
        entry_value: str | None,
        entry_flag_kind: BEnumEntryFlagKind,
        entry_sign_kind: BEnumEntrySignKind,
        entry_comment: str | None,
        hierarchy: list[str],
    ):
        super().__init__(entry_name, entry_value, entry_flag_kind, entry_sign_kind, entry_comment)
        self.__hierarchy = hierarchy

    def iter_hierarchy(self, benum: "BEnum") -> typing.Iterator["BHierarchyEnumEntry"]:
        return map(
            lambda e: typing.cast(BHierarchyEnumEntry, benum.get_entry_by_name(e)),
            self.__hierarchy,
        )

    @staticmethod
    def from_json(data: dict[str, typing.Any]) -> "BHierarchyEnumEntry":
        return BHierarchyEnumEntry(
            data["name"],
            data.get("value", None),
            BEnumEntryFlagKind(data.get("flag_kind")),
            BEnumEntrySignKind(data.get("sign_kind")),
            data.get("comment", None),
            data["hierarchy"],
        )


class BEnum:
    """The struct to describe an enum."""

    __enum_name: str
    """The name of this enum."""
    __enum_comment: str | None
    """The comment of this enum. None if no comment."""
    __is_unsigned: bool
    """True if this enum can use unsigned integer as its underlying type."""
    __is_flag: bool
    """True if this enum will use flags feature (supporting OR, AND, operators)."""
    __entries: list[BEnumEntry]
    """The list to store entries of this enum."""

    __entries_map: dict[str, BEnumEntry]
    """The name map for `entries`."""

    def __init__(
        self,
        enum_name: str,
        enum_comment: str | None,
        is_unsigned: bool,
        is_flag: bool,
        entries: list[BEnumEntry],
    ):
        self.__enum_name = enum_name
        self.__enum_comment = enum_comment
        self.__is_unsigned = is_unsigned
        self.__is_flag = is_flag
        self.__entries = entries
        self.__entries_map = {e.get_entry_name(): e for e in entries}

    def get_enum_name(self) -> str:
        """Get the name of this enum."""
        return self.__enum_name

    def get_enum_comment(self) -> str | None:
        """Get the comment of this enum. None if no comment."""
        return self.__enum_comment

    def is_unsigned(self) -> bool:
        """True if this enum can use unsigned integer as its underlying type."""
        return self.__is_unsigned

    def is_flag(self) -> bool:
        """True if this enum will use flags feature (supporting OR, AND, operators)."""
        return self.__is_flag

    def iter_entries(self) -> typing.Iterator[BEnumEntry]:
        """Get the iterator of entries of this enum."""
        return iter(self.__entries)

    def get_entry_by_name(self, name: str) -> BEnumEntry:
        return self.__entries_map[name]

    @staticmethod
    def from_json(data: dict[str, typing.Any]) -> "BEnum":
        return BEnum(
            data["name"],
            data.get("comment", None),
            data["is_unsigned"],
            data["is_flag"],
            list(map(lambda i: BEnum.__create_entry_by_content(i), data["entries"])),
        )

    @staticmethod
    def __create_entry_by_content(data: dict[str, typing.Any]) -> "BEnumEntry":
        if "hierarchy" in data:
            return BHierarchyEnumEntry.from_json(data)
        else:
            return BEnumEntry.from_json(data)


class BEnumCollection:
    """The struct to describe a collection of enums."""

    __enums: list[BEnum]
    """The list to store enums."""

    def __init__(self, enums: list[BEnum]):
        self.__enums = enums

    def iter_enums(self) -> typing.Iterator[BEnum]:
        """Get the iterator of enums."""
        return iter(self.__enums)

    def get_enums_count(self) -> int:
        """Get the count of enums."""
        return len(self.__enums)

    def get_enum_by_index(self, index: int) -> BEnum:
        """Get the enum by index."""
        return self.__enums[index]

    @staticmethod
    def from_json(data: list[typing.Any]) -> "BEnumCollection":
        return BEnumCollection(list(map(lambda i: BEnum.from_json(i), data)))


def load_enums(filename: str) -> BEnumCollection:
    with open(utils.get_input_file_path(filename), "r", encoding="utf-8") as f:
        return BEnumCollection.from_json(json.load(f))


def load_enum(filename: str) -> BEnum:
    collection = load_enums(filename)
    assert collection.get_enums_count() == 1
    return collection.get_enum_by_index(0)
